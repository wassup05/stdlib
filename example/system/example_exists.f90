program example_exists
    use stdlib_system, only: exists, type_directory, type_regular_file, &
        type_symlink
    use stdlib_error, only: state_type
    implicit none

    type(state_type) :: err
    integer :: t

    t = exists("idk", err)

    if (err%error()) then
        print *, err%print()
    end if
end program example_exists

